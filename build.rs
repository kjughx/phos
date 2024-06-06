fn main() {
    cc::Build::new()
        .file("src/idt/idt.c")
        .object("build/io/io.asm.o")
        .object("build/idt/idt.asm.o")
        .compile("idt");

    println!("cargo:rustc-link-arg-bins=--script=linker.ld",);
}
