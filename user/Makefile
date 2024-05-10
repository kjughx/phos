PROGRAMS=blank shell

all:
	mkdir -p bin/
	@for i in $(PROGRAMS); \
	do \
        make -C $$i $@; \
        cp $$i/$$i.elf bin/$$i; \
	done

clean:
	@for i in $(PROGRAMS); \
	do \
        make -C $$i $@; \
	done
	@rm -rf bin
