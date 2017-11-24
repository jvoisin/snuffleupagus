# Uncomment source, md5sums, cd to add in AUR.
pkgname="snuffleupagus"
pkgver=r142.73c30e3
pkgrel=1
pkgdesc="Snuffleupagus module for PHP7."
url="https://snuffleupagus.readthedocs.io"
arch=('i686' 'x86_64')
license=('PHP')
depends=('php')
checkdepends=('php-cgi')
#source=("$pkgname"::"git+https://github.com/nbs-system/$pkgname.git")
#md5sums=('SKIP')

pkgver() {
  #cd "$pkgname"
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
  #cd "$pkgname/src"
  phpize
  ./configure --prefix=/usr --enable-"$pkgname"
  make
}

check() {
  #cd "$pkgname/src"
  NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make test
}

package() {
  #cd "$pkgname/src"
  make INSTALL_ROOT="$pkgdir" install
  cd ..
  install -D -m644 "config/default.ini" "$pkgdir/etc/php/conf.d/$pkgname.rules"
  install -D -m644 "config/$pkgname.ini" "$pkgdir/etc/php/conf.d/$pkgname.ini"
}
