#!/bin/bash

if ! command -v gh > /dev/null; then
	echo "Install GitHub CLI tool"
	exit 1
fi

RELEASE_NAME=$(echo -n $1 | sed -e 's/^.*\///g')
BRANCH_NAME=$2
DEFAULT_BRANCH_NAME=master

if [ -z "$RELEASE_NAME" ] || [ -z "$BRANCH_NAME" ]; then
	echo "Invalid arguments"
	exit 1
fi

if [ "$BRANCH_NAME" == "$DEFAULT_BRANCH_NAME" ]; then
	RELEASE_TAG="continuous"
else
	RELEASE_TAG="continuous-$BRANCH_NAME"
fi

gh release delete "$RELEASE_TAG" \
	--yes \
	--cleanup-tag \
	--repo "$GITHUB_REPOSITORY" || true

gh run download "$GITHUB_RUN_ID" \
	--dir artifacts/ \
	--repo "$GITHUB_REPOSITORY"

pushd artifacts/ || exit 1
echo "Artifacts:"
ls
for i in $(find -mindepth 1 -maxdepth 1 -type d); do
	mv "$i"/* .
	rmdir "$i"
done
echo "Repackaged artifacts:"
ls -R
popd || exit 1

git log -1 --pretty=format:'%h%nBy %aN at %ai' > release-notes

sleep 10s
gh release create "$RELEASE_TAG" artifacts/* \
	--title "$RELEASE_NAME Continuous $BRANCH_NAME build" \
	--notes-file release-notes \
	--target "$GITHUB_SHA" \
	--repo "$GITHUB_REPOSITORY" \
	--prerelease
