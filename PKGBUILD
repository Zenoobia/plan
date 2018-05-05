# Maintainer: Your Name <joaqimpla@gmail.com>
pkgname=plan
#pkgver=$(git describe --long --tags | sed 's/\([^-]*-g\)/r\1/;s/-/./g')
pkgver=0.1
pkgrel=1
epoch=
pkgdesc="TODO: Package Description"
arch=("x86_64")
url=""
license=('GPL')
groups=()
depends=("opencv" "leptonica" "glfw" "glew" "freeglut" "glm" "tesseract-ocr-git" "argtable")
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
	cd ..
	./autogen.sh
	#cd "$pkgname-$pkgver"
	#patch -p1 -i "$srcdir/$pkgname-$pkgver.patch"
}


build() {
	../configure --prefix=/usr
	make
}

check() {
	make -k check
}

package() {
	make DESTDIR="$pkgdir/" install
}
