// Corona Hospital Management System
// Originally developed as a C++ OOP course project at Taiz University (2020).
// This maintained version preserves the console-based educational application.
// See README.md for attribution, changes, and limitations.

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace fs = std::filesystem;
namespace {

constexpr const char* kRecordVersion = "HOSPITAL_PATIENT_V1";

std::string trim(const std::string& text) {
    const auto first = text.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";
    const auto last = text.find_last_not_of(" \t\r\n");
    return text.substr(first, last - first + 1);
}

std::string lowercase(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return text;
}

// Throws on EOF so the application exits cleanly instead of looping forever.
std::string prompt(const std::string& message) {
    std::cout << message;
    std::string value;
    if (!std::getline(std::cin, value)) throw std::runtime_error("Input stream closed");
    return trim(value);
}

std::string requiredField(const std::string& label, std::size_t maxLength = 160) {
    while (true) {
        const auto value = prompt(label);
        if (value.empty()) {
            std::cout << "This field is required.\n";
        } else if (value.size() > maxLength) {
            std::cout << "Please enter at most " << maxLength << " characters.\n";
        } else if (std::any_of(value.begin(), value.end(), [](unsigned char c) {
            return c < 32 || c == 127;
        })) {
            std::cout << "Control characters are not allowed.\n";
        } else {
            return value;
        }
    }
}

int validAge() {
    while (true) {
        const std::string value = prompt("Age (0-120): ");
        if (!value.empty() && std::all_of(value.begin(), value.end(), [](unsigned char c) {
            return std::isdigit(c);
        })) {
            try {
                const unsigned long age = std::stoul(value);
                if (age <= 120) return static_cast<int>(age);
            } catch (const std::exception&) { }
        }
        std::cout << "Enter a whole number from 0 to 120.\n";
    }
}

std::string validPhone() {
    while (true) {
        auto value = requiredField("Phone number: ", 25);
        const bool allowed = std::all_of(value.begin(), value.end(), [](unsigned char c) {
            return std::isdigit(c) || c == '+' || c == '-' || c == ' ' || c == '(' || c == ')';
        });
        const auto digits = std::count_if(value.begin(), value.end(), [](unsigned char c) {
            return std::isdigit(c);
        });
        if (allowed && digits >= 3) return value;
        std::cout << "Use at least 3 digits; spaces, +, -, and parentheses are allowed.\n";
    }
}

class Patient {
public:
    Patient(std::string id, std::string first, std::string last, std::string gender,
            int age, std::string phone, std::string address, std::string problem,
            std::string doctor)
        : id_(std::move(id)), first_(std::move(first)), last_(std::move(last)),
          gender_(std::move(gender)), age_(age), phone_(std::move(phone)),
          address_(std::move(address)), problem_(std::move(problem)),
          doctor_(std::move(doctor)) {}

    const std::string& id() const { return id_; }
    const std::string& first() const { return first_; }
    const std::string& last() const { return last_; }
    const std::string& gender() const { return gender_; }
    int age() const { return age_; }
    const std::string& phone() const { return phone_; }
    const std::string& address() const { return address_; }
    const std::string& problem() const { return problem_; }
    const std::string& doctor() const { return doctor_; }

private:
    std::string id_, first_, last_, gender_;
    int age_;
    std::string phone_, address_, problem_, doctor_;
};

void printPatient(const Patient& p) {
    std::cout << "----------------------------------------\n"
              << "Patient ID: " << p.id() << '\n'
              << "Name:       " << p.first() << ' ' << p.last() << '\n'
              << "Gender:     " << p.gender() << '\n'
              << "Age:        " << p.age() << '\n'
              << "Phone:      " << p.phone() << '\n'
              << "Address:    " << p.address() << '\n'
              << "Problem:    " << p.problem() << '\n'
              << "Doctor:     " << p.doctor() << '\n';
}

// One plain-text file per ID. This remains intentionally simple for an OOP demo.
class PatientRepository {
public:
    explicit PatientRepository(fs::path base) : base_(std::move(base)), records_(base_ / "patients") {
        fs::create_directories(records_);
    }

    std::string nextId() {
        unsigned long long next = 1;
        const auto counterPath = base_ / "next_id.txt";
        if (fs::exists(counterPath)) {
            std::ifstream in(counterPath);
            std::string saved;
            if (!std::getline(in, saved) || saved.empty() ||
                !std::all_of(saved.begin(), saved.end(), [](unsigned char c) { return std::isdigit(c); })) {
                throw std::runtime_error("Invalid data/next_id.txt; verify the counter file");
            }
            try { next = std::stoull(saved); }
            catch (const std::exception&) { throw std::runtime_error("Invalid patient ID counter"); }
        }
        // Also scan records to recover from a stale counter without reusing existing IDs.
        for (const auto& entry : fs::directory_iterator(records_)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".txt") continue;
            const std::string stem = entry.path().stem().string();
            if (stem.size() < 2 || stem[0] != 'P' ||
                !std::all_of(stem.begin() + 1, stem.end(), [](unsigned char c) { return std::isdigit(c); })) continue;
            try { next = std::max(next, std::stoull(stem.substr(1)) + 1); }
            catch (const std::exception&) { /* Ignore unrelated record filenames. */ }
        }
        if (next == 0 || next >= 999999999ULL) throw std::runtime_error("Patient ID limit reached");
        // Reserve the ID first. If creating a record later fails, its number is simply skipped.
        replaceText(counterPath, std::to_string(next + 1) + "\n");
        std::ostringstream out;
        out << 'P' << std::setw(6) << std::setfill('0') << next;
        return out.str();
    }

    void save(const Patient& p) {
        const auto destination = recordPath(p.id());
        std::ostringstream out;
        out << kRecordVersion << '\n' << p.id() << '\n' << p.first() << '\n'
            << p.last() << '\n' << p.gender() << '\n' << p.age() << '\n'
            << p.phone() << '\n' << p.address() << '\n' << p.problem() << '\n'
            << p.doctor() << '\n';
        replaceText(destination, out.str());
    }

    std::optional<Patient> find(const std::string& id) const {
        const auto path = recordPath(id);
        std::ifstream in(path);
        if (!in) return std::nullopt;
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(in, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            lines.push_back(line);
        }
        if (lines.size() != 10 || lines[0] != kRecordVersion || lines[1] != id) {
            throw std::runtime_error("Invalid record format: " + path.string());
        }
        try {
            std::size_t consumed = 0;
            const int age = std::stoi(lines[5], &consumed);
            if (consumed != lines[5].size() || age < 0 || age > 120) throw std::invalid_argument("age");
            return Patient(lines[1], lines[2], lines[3], lines[4], age,
                           lines[6], lines[7], lines[8], lines[9]);
        } catch (const std::exception&) {
            throw std::runtime_error("Invalid patient age in " + path.string());
        }
    }

    std::vector<Patient> all() const {
        std::vector<Patient> result;
        for (const auto& entry : fs::directory_iterator(records_)) {
            if (!entry.is_regular_file() || entry.path().extension() != ".txt") continue;
            try {
                auto record = find(entry.path().stem().string());
                if (record) result.push_back(*record);
            } catch (const std::exception& e) {
                std::cerr << "Warning: skipped " << entry.path().filename().string()
                          << " (" << e.what() << ")\n";
            }
        }
        std::sort(result.begin(), result.end(), [](const Patient& a, const Patient& b) {
            return a.id() < b.id();
        });
        return result;
    }

    bool remove(const std::string& id) {
        return fs::remove(recordPath(id));
    }

private:
    fs::path base_, records_;

    fs::path recordPath(const std::string& id) const {
        if (id.size() < 2 || id[0] != 'P' ||
            !std::all_of(id.begin() + 1, id.end(), [](unsigned char c) { return std::isdigit(c); })) {
            throw std::invalid_argument("Invalid patient ID. Use the ID displayed in the list.");
        }
        return records_ / (id + ".txt");
    }

    static void replaceText(const fs::path& dest, const std::string& contents) {
        const fs::path temp = dest.string() + ".tmp";
        const fs::path backup = dest.string() + ".bak";
        if (fs::exists(backup)) {
            throw std::runtime_error("Recovery backup exists: " + backup.string() +
                                     ". Check it before saving again.");
        }
        {
            std::ofstream out(temp, std::ios::trunc | std::ios::binary);
            if (!out) throw std::runtime_error("Cannot write: " + temp.string());
            out << contents;
            out.flush();
            if (!out) {
                out.close();
                fs::remove(temp);
                throw std::runtime_error("Write failed: " + temp.string());
            }
        }
        std::error_code ec;
        const bool existed = fs::exists(dest);
        if (existed) {
            fs::rename(dest, backup, ec);
            if (ec) {
                fs::remove(temp);
                throw std::runtime_error("Cannot back up record: " + ec.message());
            }
        }
        fs::rename(temp, dest, ec);
        if (ec) {
            if (existed) {
                std::error_code rollbackError;
                fs::rename(backup, dest, rollbackError);
                if (rollbackError) {
                    throw std::runtime_error("Save failed; recover from: " + backup.string());
                }
            }
            fs::remove(temp);
            throw std::runtime_error("Cannot save record: " + ec.message());
        }
        if (existed) {
            std::error_code cleanupError;
            fs::remove(backup, cleanupError);
            if (cleanupError) {
                std::cerr << "Warning: stale backup left at " << backup.string() << '\n';
            }
        }
    }
};

class Operations {
public:
    explicit Operations(fs::path dataDirectory) : repository_(std::move(dataDirectory)) {}

    void addPatient() {
        std::cout << "\n--- Add patient ---\n";
        // Gather every field before touching disk, so cancellation/EOF does not create a partial record.
        const std::string first = requiredField("First name: ", 80);
        const std::string last = requiredField("Last name: ", 80);
        const std::string gender = requiredField("Gender: ", 40);
        const int age = validAge();
        const std::string phone = validPhone();
        const std::string address = requiredField("Address: ");
        const std::string problem = requiredField("Problem: ");
        const std::string doctor = requiredField("Doctor: ");
        const auto id = repository_.nextId();
        const Patient patient(id, first, last, gender, age, phone, address, problem, doctor);
        repository_.save(patient);
        std::cout << "Patient added successfully. ID: " << id << "\n";
    }

    void listPatients() const {
        std::cout << "\n--- Patient list ---\n";
        const auto patients = repository_.all();
        if (patients.empty()) {
            std::cout << "No patients found.\n";
            return;
        }
        for (const auto& patient : patients) printPatient(patient);
        std::cout << "----------------------------------------\n"
                  << "Total patients: " << patients.size() << "\n";
    }

    void searchPatient() const {
        std::cout << "\n--- Search patients ---\n";
        const std::string query = lowercase(requiredField("Patient ID or name: ", 160));
        std::size_t matches = 0;
        for (const auto& p : repository_.all()) {
            const std::string fullName = lowercase(p.first() + " " + p.last());
            if (lowercase(p.id()) == query || fullName.find(query) != std::string::npos) {
                printPatient(p);
                ++matches;
            }
        }
        if (!matches) std::cout << "No matching patients found.\n";
        else std::cout << "Matches: " << matches << "\n";
    }

    void editPatient() {
        std::cout << "\n--- Modify patient ---\n";
        const std::string id = requiredField("Patient ID to modify: ", 20);
        const auto patient = repository_.find(id);
        if (!patient) {
            std::cout << "Patient not found.\n";
            return;
        }
        printPatient(*patient);
        std::cout << "Enter all replacement values. The ID stays the same.\n";
        const std::string first = requiredField("First name: ", 80);
        const std::string last = requiredField("Last name: ", 80);
        const std::string gender = requiredField("Gender: ", 40);
        const int age = validAge();
        const std::string phone = validPhone();
        const std::string address = requiredField("Address: ");
        const std::string problem = requiredField("Problem: ");
        const std::string doctor = requiredField("Doctor: ");
        repository_.save(Patient(id, first, last, gender, age, phone, address, problem, doctor));
        std::cout << "Patient modified successfully.\n";
    }

    void deletePatient() {
        std::cout << "\n--- Delete patient ---\n";
        const std::string id = requiredField("Patient ID to delete: ", 20);
        const auto patient = repository_.find(id);
        if (!patient) {
            std::cout << "Patient not found.\n";
            return;
        }
        printPatient(*patient);
        if (lowercase(prompt("Confirm deletion by typing YES: ")) != "yes") {
            std::cout << "Deletion canceled.\n";
            return;
        }
        if (repository_.remove(id)) std::cout << "Patient deleted successfully.\n";
        else std::cout << "Patient could not be deleted.\n";
    }

private:
    PatientRepository repository_;
};

void displayHeader() {
    std::cout << "===============================================\n"
              << "        CORONA HOSPITAL MANAGEMENT SYSTEM\n"
              << "===============================================\n"
              << "Taiz University | College of Engineering | IT\n"
              << "Original OOP project contributors:\n"
              << "  Omar Yahya Saeed\n"
              << "  Hazim Saeed Alqady\n"
              << "  Shehab Mohammed Al-Ward\n"
              << "Educational demonstration only; no real patient data.\n";
}

void displayMenu() {
    std::cout << "\n1. Add a new patient\n"
              << "2. Modify patient information\n"
              << "3. Delete patient information\n"
              << "4. Search for a patient\n"
              << "5. List all patients\n"
              << "0. Exit\n";
}

} // namespace

int main() {
    try {
        const char* configured = std::getenv("HOSPITAL_DATA_DIR");
        fs::path dataDirectory = configured && *configured ? fs::path(configured) : fs::path("data");
        Operations operations(dataDirectory);
        displayHeader();
        while (true) {
            displayMenu();
            const std::string choice = prompt("Enter your choice: ");
            try {
                if (choice == "0") { std::cout << "Goodbye!\n"; return 0; }
                if (choice == "1") operations.addPatient();
                else if (choice == "2") operations.editPatient();
                else if (choice == "3") operations.deletePatient();
                else if (choice == "4") operations.searchPatient();
                else if (choice == "5") operations.listPatients();
                else std::cout << "Enter a number between 0 and 5.\n";
            } catch (const std::runtime_error& e) {
                if (std::string(e.what()) == "Input stream closed") {
                    std::cout << "\nInput closed. Goodbye!\n";
                    return 0;
                }
                std::cerr << "Error: " << e.what() << '\n';
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << '\n';
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << '\n';
        return 1;
    }
}
