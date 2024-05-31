import subprocess
import os
import random
from datetime import datetime, timedelta

START_DATE = datetime(2021, 3, 14)
END_DATE = datetime.now()

TYPES = ["fix", "feat", "chore", "docs", "refactor", "style", "test", "perf"]
AREAS = [
    "kernel", "memory", "io", "network", "security", "init", "filesystem",
    "driver", "config", "api", "scheduler", "logging"
]
ACTIONS = [
    "add", "improve", "rewrite", "refactor", "document", "cleanup", "split"
]
SUBJECTS = [
    "interrupts", "page tables", "syscalls", "startup config", "memory map",
    "device handlers", "scheduler logic", "TCP stack", "filesystem hooks",
    "boot process", "log rotation", "signal handling"
]

def generate_commit_message():
    return f"{random.choice(TYPES)}({random.choice(AREAS)}): {random.choice(ACTIONS)} {random.choice(SUBJECTS)}"

def generate_random_date():
    delta = END_DATE - START_DATE
    random_offset = timedelta(days=random.randint(0, delta.days),
                              hours=random.randint(0, 23),
                              minutes=random.randint(0, 59),
                              seconds=random.randint(0, 59))
    return START_DATE + random_offset

def commit_file(file_path):
    commit_time = generate_random_date().strftime("%Y-%m-%dT%H:%M:%S")
    commit_msg = generate_commit_message()

    env = os.environ.copy()
    env["GIT_AUTHOR_DATE"] = commit_time
    env["GIT_COMMITTER_DATE"] = commit_time
    env["GIT_AUTHOR_NAME"] = "Ryan"
    env["GIT_AUTHOR_EMAIL"] = "dev@example.com"
    env["GIT_COMMITTER_NAME"] = "Ryan"
    env["GIT_COMMITTER_EMAIL"] = "dev@example.com"

    subprocess.run(["git", "add", file_path], check=True)
    subprocess.run(["git", "commit", "-m", commit_msg], env=env, check=True)

def main():
    if not os.path.exists(".git"):
        print("❌ Not a Git repo.")
        return

    # Get untracked files only
    result = subprocess.run(["git", "ls-files", "--others", "--exclude-standard"],
                            capture_output=True, text=True)
    files = result.stdout.strip().splitlines()
    files = [f for f in files if os.path.isfile(f)]

    print(f"📁 Found {len(files)} files to commit.")

    for i, file_path in enumerate(sorted(files)):
        print(f"🔧 Committing {file_path} ({i+1}/{len(files)})")
        commit_file(file_path)

    print("\n✅ All files committed individually with randomized timestamps.")

if __name__ == "__main__":
    main()

