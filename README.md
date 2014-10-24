gui
===

Sources for building the app.

Notes:

Version.cpp is required for the build but it is not in the repository as it is created everytime a commit is made.
The 'post-commit' hook has this:

#!/bin/bash
revisioncount=`git log --oneline | wc -l`
projectversion=`git describe --tags --long`
cleanversion=${projectversion%%-*}
version="version.cpp"
echo "#include <QString>" > $version
echo "QString buildVersion() {"  >> $version
echo "     return  \"$projectversion-$revisioncount\";" >> $version
echo "}" >> $version

