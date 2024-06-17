Installation lib Allegro
Installing Allegro as a binary package is the easiest method for most operating systems.

Choose your weapon:

Linux
Ubuntu 18.04+ (or derivatives thereof)
First, add the Allegro PPA. This gives you up-to-date versions of Allegro; the base repos only provide 5.2.3 at the time of writing.

sudo add-apt-repository ppa:allegro/5.2
Then, install Allegro:

sudo apt-get install liballegro*5.2 liballegro*5-dev

Debian-based distributions (Debian, Mint, Ubuntu, etc..)
sudo apt-get install liballegro-ttf5-dev

Fedora
sudo dnf install allegro5*

Arch
sudo pacman -S allegro
openSUSE Tumbleweed
sudo zypper install liballegro*

Após a instalação da biblioteca allegro, execute o comando "make" e em seguida "./a4" e se divirta.