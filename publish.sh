#
# use this to 'publish' the documentation
#
#  1. remote    - will end on http://laneslexicon.github.io/lexicon/site/
#
#  2. local     - the in program documentation
#
#  3. config    - the in program documentation copy in the clean config repo
#
projectroot=~/qt5projects/mansur
#
# remote, You must be in the gh-pages branch of lexicon repository
#
function remote {
branch=`git -C $projectroot/gui rev-parse --abbrev-ref HEAD`
if [ "$branch" != "gh-pages" ]; then
    echo "You must be in gh-pages in the lexicon repo aborting"
    exit
fi
rsync  -avz $projectroot/documentation/site/ $projectroot/gui/site
perl $projectroot/documentation/ar_markup.pl --site $projectroot/gui/site --verbose
}
#
#  local updates the in program documentation in gui/Resources/site
#
function local {
echo "Updating local docs.."
rsync -avz $projectroot/documentation/site $projectroot/gui/Resources
perl $projectroot/documentation/ar_markup.pl --site $projectroot/gui/Resources/site --verbose
find $projectroot/gui/Resources/site -iname "*.html" -exec perl -i -ne 'print unless /googleapis.com/' {} \;
}
#
# the config repository is the for clean config
#
#
function config {
echo "Updating config repo.."
rsync -avz $projectroot/documentation/site/ $projectroot/config/site
perl $projectroot/documentation/ar_markup.pl --site $projectroot/config/site --verbose
find $projectroot/gui/Resources/site -iname "*.html" -exec perl -i -ne 'print unless /googleapis.com/' {} \;
}

remote
echo "you did do a  mkdocs build --clean   ?????"
