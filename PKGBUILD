# Maintainer:  fernando villarreal <fernando.villarreal.sd@gmail.co>

pkgname=s3fs-fuse-rc4
pkgver=1.89.r1875.6edd47e
pkgrel=1
pkgdesc='FUSE-based file system backed by Amazon S3 with OpenSSL support, with rc4 encryption'
arch=('i686' 'x86_64')
url='https://github.com/delinquent313/s3fs-fuse-rc4'
license=('GPL2')
depends=('fuse>=2.8.4'
         'curl'
         'libxml2')
source=('git+https://github.com/delinquent313/s3fs-fuse-rc4')
sha256sums=('SKIP')
makedepends=('git'
             'glib2'
             'mime-types'
             'libxml++'
             'libgcrypt'
             'openssl>=0.9.8')
provides=('s3fs-fuse' 's3fs')
conflicts=('s3fs-fuse' 's3fs' 's3fs-c-git')

pkgver() {
  cd s3fs-fuse-rc4
  printf "%s." "$(grep "AC_INIT(" configure.ac | awk 'BEGIN {FS = " "} {print $2}')" |  sed 's/)//g'
  printf "r%s.%s" "$(git rev-list --count HEAD)" "$(git rev-parse --short HEAD)"
}

build() {
  cd s3fs-fuse-rc4
  ./autogen.sh
  ./configure --prefix=/usr --with-openssl
  make
}

package() {
  cd s3fs-fuse-rc4

  # We don't need anything related to git in the package
  rm -rf .git*

  # Install License
  install -D -m644 COPYING "${pkgdir}/usr/share/licenses/${pkgname}/LICENSE"

  # Install Documentation
  install -D -m644 README.md "${pkgdir}/usr/share/doc/${pkgname}/README"

  # make install
  make DESTDIR="${pkgdir}/" install
}

