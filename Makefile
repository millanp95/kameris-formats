.PHONY: all pip-package pip-distribute distribute clean lint


all: pip-package


pip-package: clean
	cd python && python setup.py sdist
	pip install --upgrade wheel
	cd python && python setup.py bdist_wheel --universal

pip-distribute: pip-package
	pip install --upgrade twine
	cd python && python -m twine upload dist/*.whl dist/*.tar.*

distribute: pip-distribute


clean:
	rm -rf build dist *.egg-info && find . -type f \( -name '*.pyc' -o -name '*.pyo' \) -delete


lint:
	python -m flake8 python/
