fn main() {
    cc::Build::new()
        .flag("-nostdlib")
        .file("src/idt/idt.c")
        .object("build/io/io.asm.o")
        .object("build/idt/idt.asm.o")
        .compile("idt");

    cc::Build::new()
        .flag("-nostdlib")
        .file("src/memory/memory.c")
        .compile("memory");

    cc::Build::new()
        .flag("-nostdlib")
        .file("src/gdt/gdt.c")
        .object("build/gdt/gdt.asm.o")
        .compile("gdt");

    println!("cargo:rustc-link-arg-bins=--script=linker.ld",);
}
