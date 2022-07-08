# MY HOMEBREW CONFIG

DIR="~/goinfre"
STATUS_FILE="$HOME/brew_status"

if [[ -e $DIR/.brew ]]
then
	echo "$DIR/.brew already installed" > $STATUS_FILE
	exit 0
fi


git clone --depth=1 https://github.com/Homebrew/brew $DIR/.brew

# Create .brewconfig script in home directory
cat > $DIR/.brewconfig.zsh << EOL

# Add brew to path
export PATH=\$DIR/.brew/bin:\$PATH

# Set Homebrew temporary folders
export HOMEBREW_CACHE=/tmp/\$USER/Homebrew/Caches
export HOMEBREW_TEMP=/tmp/\$USER/Homebrew/Temp

mkdir -p \$HOMEBREW_CACHE
mkdir -p \$HOMEBREW_TEMP

# If NFS session
# Symlink Locks folder in /tmp
if df -T autofs,nfs \$DIR 1>/dev/null
then
  HOMEBREW_LOCKS_TARGET=/tmp/\$USER/Homebrew/Locks
  HOMEBREW_LOCKS_FOLDER=\$DIR/.brew/var/homebrew

  mkdir -p \$HOMEBREW_LOCKS_TARGET
  mkdir -p \$HOMEBREW_LOCKS_FOLDER

  # Symlink to Locks target folders
  # If not already a symlink
  if ! [[ -L \$HOMEBREW_LOCKS_FOLDER && -d \$HOMEBREW_LOCKS_FOLDER ]]
  then
     echo "Creating symlink for Locks folder"
     rm -rf \$HOMEBREW_LOCKS_FOLDER
     ln -s \$HOMEBREW_LOCKS_TARGET \$HOMEBREW_LOCKS_FOLDER
  fi
fi
EOL

# Add .brewconfig sourcing in your .zshrc if not already present
if ! grep -q "# Load Homebrew config script" $HOME/.zshrc
then
cat >> $HOME/.zshrc << EOL
# Load Homebrew config script
DIR="~/goinfre"
source $DIR/.brewconfig.zsh
EOL
fi
