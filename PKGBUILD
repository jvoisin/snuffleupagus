# Uncomment source, md5sums, cd to add in AUR.
pkgname=snuffleupagus
pkgver=r140.2d9b80c
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
  #git checkout archlinux_pkg >/dev/null

  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
  #cd "$pkgname/src"

  phpize
  ./configure --prefix=/usr --enable-snuffleupagus
  make
}

check() {
  #cd "$pkgname/src"

  # These tests actually fail:
  # Harden mt_rand [tests/harden_mt_rand.phpt]
  # Harden rand [tests/harden_rand.phpt]
  NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make test
}

package() {
  #cd "$pkgname/src"
  make INSTALL_ROOT=$pkgdir install

  cd ..
  install -D -m644 config/default.ini $pkgdir/etc/php/conf.d/$pkgname.rules
  install -D -m644 config/snuffleupagus.ini $pkgdir/etc/php/conf.d/$pkgname.ini
}
