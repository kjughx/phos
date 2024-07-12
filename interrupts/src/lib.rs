use proc_macro::TokenStream;
use quote::quote;
use syn::{parse_macro_input, LitInt};

#[proc_macro]
pub fn interrupt_table(input: TokenStream) -> TokenStream {
    let input = parse_macro_input!(input as LitInt);
    let num_interrupts = input.base10_parse::<usize>().unwrap();

    let mut interrupt_fns = Vec::new();
    let mut interrupt_entries = Vec::new();

    for i in 0..num_interrupts {
        let fn_name = syn::Ident::new(&format!("int{}", i), proc_macro2::Span::call_site());
        let i_str = i.to_string();
        interrupt_fns.push(quote! {
            #[naked]
            #[no_mangle]
            pub extern "C" fn #fn_name() {
                unsafe {
                    asm!(
                        ".code32",
                        "pushad", // Push general purpose registers
                        "push esp",
                        concat!("push ", #i_str),
                        "call interrupt_handler",
                        "add esp, 8", // Pop the stack
                        "popad", // Restore general purpose registers
                        "iret", // Return from interrupt
                        options(noreturn)
                    );
                }
            }
        });

        interrupt_entries.push(quote! {
            #fn_name,
        });
    }

    let expanded = quote! {
        #(#interrupt_fns)*

        #[link_section = ".data"]
        #[no_mangle]
        static INTERRUPT_POINTER_TABLE: [unsafe extern "C" fn(); #num_interrupts] = [
            #(#interrupt_entries)*
        ];
    };

    TokenStream::from(expanded)
}

