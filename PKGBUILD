pkgname=snuffleupagus
pkgver=0.0
pkgrel=1
arch=('i686' 'x86_64')
license=('PHP')
depends=('php')
checkdepends=('php-cgi')
source=("$pkgname"::"git+https://github.com/nbs-system/$pkgname.git")
md5sums=('SKIP')

build() {
  cd $pkgname/src

  # Remove before merge
  git checkout archlinux_pkg

  phpize
  ./configure --prefix=/usr --enable-snuffleupagus
  make
}

check() {
  cd $pkgname/src
  # These tests actually fail:
  # Harden mt_rand [tests/harden_mt_rand.phpt]
  # Harden rand [tests/harden_rand.phpt]
  NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make test
}

package() {
  cd $pkgname/src
  make INSTALL_ROOT=$pkgdir install

  cd ..
  install -D -m644 config/default.ini $pkgdir/etc/php/conf.d/snuffleupagus.rules
  install -D -m644 config/snuffleupagus.ini $pkgdir/etc/php/conf.d/$pkgname.ini
}
