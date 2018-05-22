#!/bin/bash

git add .
git status
echo "Commit message: "
read message
git commit -m "$message"