fn main() {
    cc::Build::new()
        .flag("-nostdlib")
        .file("src/c/idt.c")
        .object("build/asm/io.asm.o")
        .object("build/asm/idt.asm.o")
        .compile("idt");

    cc::Build::new()
        .flag("-nostdlib")
        .file("src/c/memory.c")
        .compile("memory");

    cc::Build::new()
        .flag("-nostdlib")
        .file("src/c/gdt.c")
        .object("build/asm/gdt.asm.o")
        .compile("gdt");

    println!("cargo:rustc-link-arg-bins=--script=linker.ld",);
}
