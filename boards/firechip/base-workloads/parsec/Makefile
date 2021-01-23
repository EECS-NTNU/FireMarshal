.PHONY: overlay

SUITE := parsec
ARCH := riscv-64

overlay: overlay/opt/$(SUITE)/extracted

$(SUITE).tar.zst:
	@echo "Please provide the $@ archive from the script author!" >&2
	@exit 1

overlay/opt/$(SUITE)/extracted: $(SUITE).tar.zst
	mkdir -p $$(dirname $@)
	# Extract
	tar --use-compress-program=zstd --skip-old-files -xf $(SUITE).tar.zst -C overlay/opt
	# Cleanup
	find overlay/opt/$(SUITE) -mindepth 2 -maxdepth 2 -type f ! -name '*$(ARCH)' -exec rm -v {} \; || true
	# Touch extracted target
	touch $@

clean:
	rm -Rf overlay/opt/$(SUITE)
	git checkout overlay/opt/
