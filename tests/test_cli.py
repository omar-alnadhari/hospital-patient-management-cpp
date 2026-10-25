"""Small integration checks for the educational patient-manager CLI.

Usage: python3 tests/test_cli.py /path/to/Hospital
"""
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def run(binary: Path, folder: Path, lines: list[str]) -> str:
    environment = os.environ.copy()
    environment["HOSPITAL_DATA_DIR"] = str(folder / "private-test-data")
    result = subprocess.run(
        [str(binary)], input="\n".join(lines) + "\n",
        capture_output=True, text=True, env=environment, cwd=folder, timeout=10,
    )
    if result.returncode != 0:
        raise AssertionError(f"Exit {result.returncode}\nSTDOUT:\n{result.stdout}\nSTDERR:\n{result.stderr}")
    return result.stdout + result.stderr


def person(first: str, last: str, age: str = "25", problem: str = "Demo problem") -> list[str]:
    return [first, last, "Female", age, "+39 123 456 789", "Demo Road 1", problem, "Dr Example"]


def main(binary: Path) -> None:
    with tempfile.TemporaryDirectory(prefix="hospital-test-") as tmp:
        folder = Path(tmp)
        out = run(binary, folder, ["1", *person("Alex", "Alpha"), "1", *person("Alex", "Beta"), "5", "0"])
        assert "Patient added successfully. ID: P000001" in out
        assert "Patient added successfully. ID: P000002" in out
        assert "Total patients: 2" in out
        assert "Alex Alpha" in out and "Alex Beta" in out
        assert (folder / "private-test-data/patients/P000001.txt").is_file()
        assert (folder / "private-test-data/patients/P000002.txt").is_file()

        out = run(binary, folder, ["4", "Alex", "2", "P000001", *person("Taylor", "Alpha", "33", "Updated example"),
                                   "4", "P000001", "3", "P000002", "no", "5", "0"])
        assert "Matches: 2" in out
        assert "Patient modified successfully." in out
        assert "Taylor Alpha" in out and "Updated example" in out
        assert "Deletion canceled." in out
        assert "Total patients: 2" in out

        out = run(binary, folder, ["3", "P000002", "YES", "5", "1", *person("Sam", "Demo", "999", "Test"), "0"])
        # Age 999 was rejected. The next value (Test) is rejected too; EOF ends cleanly.
        assert "Patient deleted successfully." in out
        assert "Total patients: 1" in out
        assert "Enter a whole number from 0 to 120." in out
        assert not (folder / "private-test-data/patients/P000002.txt").exists()

        # A canceled/incomplete input sequence must not create a new patient.
        out = run(binary, folder, ["5", "0"])
        assert "Total patients: 1" in out

        out = run(binary, folder, ["1", *person("Alex", "Again", "20"), "5", "0"])
        assert "Patient added successfully. ID: P000003" in out
        assert "Total patients: 2" in out

        out = run(binary, folder, ["3", "INVALID", "4", "No One", "0"])
        assert "Invalid patient ID" in out
        assert "No matching patients found." in out

        print("PASS: persistence, add, list, search, edit, delete, confirmation, validation, unique IDs")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        raise SystemExit("Usage: python3 tests/test_cli.py PATH_TO_HOSPITAL_BINARY")
    binary_path = Path(sys.argv[1]).resolve()
    if not binary_path.is_file():
        raise SystemExit(f"Binary not found: {binary_path}")
    main(binary_path)
