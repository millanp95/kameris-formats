from setuptools import setup, find_packages


setup(
    name='modmap-generator-formats',
    version='0.1.dev1',
    description=('Reader and writer implementations for the file formats used by modmap-generator-cpp.'),
    classifiers=[
        'Development Status :: 4 - Beta',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.4',
        'Programming Language :: Python :: 3.5',
        'Programming Language :: Python :: 3.6',
        'Topic :: Scientific/Engineering'
    ],
    author='Stephen',
    author_email='stephsolis@gmail.com',
    url='https://github.com/stephensolis/modmap-generator-formats/',
    license='MIT',
    packages=find_packages(),
    install_requires=[
        'numpy'
    ]
)
