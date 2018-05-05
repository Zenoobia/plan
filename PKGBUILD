# This is an example PKGBUILD file. Use this as a start to creating your own,
# and remove these comments. For more information, see 'man PKGBUILD'.
# NOTE: Please fill out the license field for your package! If it is unknown,
# then please put 'unknown'.

# Maintainer: Your Name <youremail@domain.com>
pkgname=helloWorld
pkgver=0.1
pkgrel=1
epoch=
pkgdesc=""
arch=("x86_64")
url=""
license=('GPL')
groups=()
depends=()
makedepends=()
checkdepends=()
optdepends=()
provides=()
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
#source=("$pkgname-$pkgver.tar.gz")
        #"$pkgname-$pkgver.patch")
#source=("ftp://ftp.gnu.org/gnu/sed/$pkgname-$pkgver.tar.xz")
noextract=()
md5sums=()
validpgpkeys=()


prepare() {
#mkdir build
	cd ..
	./autogen.sh
	#cd "$pkgname-$pkgver"
	#patch -p1 -i "$srcdir/$pkgname-$pkgver.patch"
}


build() {
cd ..
	./configure --prefix=/usr
	make
}

check() {
cd ..
	make -k check
}

package() {
cd ..
	make DESTDIR="$pkgdir/" install
}
