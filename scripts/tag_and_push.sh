#!/usr/bin/env bash
set -e

VERSION_FILE="VERSION"
VERSION=$(cat "$VERSION_FILE" | tr -d '\n')
TAG="v$VERSION"

# 1. Check for existing tag
git fetch --tags
if git tag | grep -q "^$TAG$"; then
  echo "[ERROR] Tag $TAG already exists. Please update the version in the VERSION file."
  exit 1
fi

# 2. Check for uncommitted changes (warning + prompt)
if ! git diff --quiet || ! git diff --cached --quiet; then
  echo "[WARNING] There are uncommitted changes."
  echo "Do you want to continue tagging and pushing? (y/N): "
  read -r yn
  case $yn in
    [yY]*)
      echo "Continuing."
      ;;
    *)
      echo "Aborted. Please commit your changes and try again."
      exit 1
      ;;
  esac
fi

# 3. Check if VERSION file content is included in the latest commit
git show HEAD:$VERSION_FILE | grep -q "^$VERSION$" || {
  echo "[ERROR] The content of the VERSION file is not included in the latest commit. You may have forgotten to commit."
  exit 1
}

# 4. Tag and push
git tag "$TAG"
git push origin "$TAG"
echo "[OK] Tag $TAG has been created and pushed."
