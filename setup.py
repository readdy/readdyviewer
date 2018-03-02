import os
import re
import sys
import platform
import subprocess

from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
from distutils.version import LooseVersion

__version__ = '0.0.1'


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        extdir = os.path.join(extdir, "readdyviewer")
        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DGLFW_BUILD_EXAMPLES=OFF',
                      '-DGLFW_BUILD_TESTS=OFF',
                      '-DGLFW_BUILD_DOCS=OFF',
                      '-DOPTION_BUILD_EXAMPLES=OFF',
                      '-DOPTION_BUILD_TESTS=OFF',
                      '-DOPTION_BUILD_GPU_TESTS=OFF',
                      '-DOPTION_BUILD_TOOLS=OFF',
                      '-DCMAKE_INSTALL_PREFIX=' + os.environ['PREFIX']]

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2 ** 32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j4']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        # subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)
        subprocess.check_call(['make', 'readdyviewer_binding'] + build_args, cwd=self.build_temp)
        subprocess.check_call(['make', 'install'], cwd=self.build_temp)


def package_files(directory):
    paths = []
    for (path, directories, filenames) in os.walk(directory):
        for filename in filenames:
            paths.append(os.path.join('..', path, filename))
    return paths


extra_files = package_files('readdyviewer/resources')

print("------- using extra files ---------")
for f in extra_files:
    print("\t{}".format(f))

setup(
    name='ReaDDyViewer',
    version=__version__,
    author='Moritz Hoffmann',
    author_email='clonker at gmail.com',
    url='https://github.com/readdy/readdyviewer',
    description='',
    long_description='',
    ext_modules=[CMakeExtension('readdyviewer_binding')],
    packages=find_packages(),
    zip_safe=False,
    cmdclass=dict(build_ext=CMakeBuild),
    package_data={'readdyviewer': extra_files},
    install_requires=['numpy']
)
