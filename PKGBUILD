# Maintainer NBS System Security Team <secu@nbs-system.com>
pkgname="snuffleupagus"
pkgver=r146.baa0d6d
pkgrel=1
pkgdesc="Security module for PHP7."
url="https://snuffleupagus.readthedocs.io"
arch=('i686' 'x86_64')
license=('LGPL3')
depends=('php' 'php-fpm')
checkdepends=()
source=("${pkgname}"::"git+https://github.com/nbs-system/${pkgname}.git")
md5sums=('SKIP')

pkgver() {
  cd "${srcdir}/${pkgname}"
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
  cd "${srcdir}/${pkgname}/src"
  phpize
  ./configure --prefix=/usr --enable-"${pkgname}"
  make
}

check() {
  NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make -C "${srcdir}/${pkgname}/src" test
}

package() {
  make -C "${srcdir}/${pkgname}/src" INSTALL_ROOT="${pkgdir}" install
  install -D -m644 "${srcdir}/${pkgname}/config/default.ini" "${pkgdir}/etc/php/conf.d/${pkgname}.rules"
  install -D -m644 "${srcdir}/${pkgname}/config/${pkgname}.ini" "${pkgdir}/etc/php/conf.d/${pkgname}.ini"
}
