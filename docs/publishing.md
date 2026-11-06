# GitHub repository

The project is hosted at [B1-Mordred/B1-Focuser](https://github.com/B1-Mordred/B1-Focuser), on branch `main`.

Clone with `gh repo clone B1-Mordred/B1-Focuser` or `git clone https://github.com/B1-Mordred/B1-Focuser.git`.

Review and commit:

```bash
git status
git add .
git diff --cached --stat
git diff --cached
git commit -m "Update B1-Focuser firmware and documentation"
```

Push committed changes to the existing repository:

```bash
git push origin main
```

Build products, local logs, editor state, and environment files are ignored. Do not commit Wi-Fi passwords, host credentials, or device-specific secrets. Firmware needs no network credentials.

GitHub Actions compiles the firmware and runs manual-switch logic tests on pushes and pull requests. Its result does not certify physical hardware operation.
