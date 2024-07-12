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
        .object("build/asm/paging.asm.o")
        .compile("memory");

    cc::Build::new()
        .object("build/asm/tss.asm.o")
        .object("build/asm/task.asm.o")
        .compile("task");

    if std::env::var("PROFILE").unwrap() == "debug" {
        println!("cargo:rustc-cfg=feature=\"trace\"");
    }

    println!("cargo:rustc-link-arg-bins=--script=linker.ld",);
}
