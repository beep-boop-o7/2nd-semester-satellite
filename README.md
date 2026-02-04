# 2nd-semester-satellite

## Includes
Put your nonstandard library includes here (the ones you had to install separately)
available in the arduino ide unless otherwise noted

## How to use git (and github)
make sure git is installed.

### Setup

initilize a git instance in the directory(folder) that you want the project stored in
```
git init
```

clone the repo(repository) (copy it to your computer)
```
git clone https://github.com/beep-boop-o7/2nd-semester-satellite.git
```

set repo as origin
```
git remote add origin https://github.com/beep-boop-o7/2nd-semester-satellite.git
```

### Using

make a new branch for the feature you are working on (only do this for new features)
```
git checkout -b feature/feature_name
```

commit the changes you have made to the local branch (with a brief description)
```
git commit -m "This is a brief description"
```

push your local branch upstream (to github)
```
git push -u origin feature/feature_name
```
after that you go into github to make a merge request to merge your branch into main

to pull the latest changes from main
```
git pull
```

if git complains about unsaved changes
```
git stash
```

a helpful resource 
https://education.github.com/git-cheat-sheet-education.pdf
