# Maintained by Julien (jvoisin) Voisin <julien.voisin+snuffleupagus@dustri.org>
pkgname="php7-snuffleupagus"
_pkgname="snuffleupagus"
pkgver=0.1
pkgrel=1
pkgdesc="Snuffleupagus module for PHP7."
url="https://snuffleupagus.readthedocs.io"
arch="all"
license="LGPL3"
depends="php7-dev php7-fpm"
source=""

_giturl="https://github.com/jvoisin/snuffleupagus.git"

prepare() {
  default_prepare
  # Use tarball instead of git for release.
  # https://wiki.alpinelinux.org/wiki/Creating_an_Alpine_package#APKBUILD_variables.2Ffunctions
  git clone "${_giturl}" "${srcdir}/${_pkgname}" || return 1
}

build() {
  cd "${srcdir}/${_pkgname}/src"
  phpize || return 1
  ./configure --prefix=/usr --enable-"${_pkgname}" || return 1
  make || return 1
}

check() {
  NO_INTERACTION=1 REPORT_EXIT_STATUS=1 make -C "${srcdir}/${_pkgname}/src" test
}

package() {
  make -C "${srcdir}/${_pkgname}/src" INSTALL_ROOT="${pkgdir}" install || return 1
  install -D -m644 "${srcdir}/${_pkgname}/config/default.rules" "${pkgdir}/etc/php/conf.d/${_pkgname}.rules" || return 1
  install -D -m644 "${srcdir}/${_pkgname}/config/${_pkgname}.rules" "${pkgdir}/etc/php7/conf.d/${_pkgname}.rules" || return 1
}
