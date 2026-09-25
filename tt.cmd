git checkout master
git pull
git checkout develop
git merge --strategy=ours master
git checkout master
git merge develop
