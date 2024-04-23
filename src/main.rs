use core::str;
use std::io::{self, Write};

use clearscreen::*;
use colored::*;
use sys_info::os_type;
fn main() {
    clear().unwrap();
    const VER: &str = "1.0.0";
    match os_type() {
        Ok(os) => match os.as_str() {
            "Linux" => println!(
                "{}{}",
                "Welcome to Halsh!\nA Beautifull Shell For Linux!\nHalsh Version - ".white(),
                VER.white()
            ),
            "Windows" => println!(
                "{}{}",
                "Welcome to Halsh!\nA Beautifull Shell For Windows!\nHalsh Version - ", VER
            ),
            _ => println!("Well.yeah!"),
        },
        Err(os) => {
            println!("Failed Getting Os Type : {}", os);
        }
    }

    loop {
        print!(" : ");
        io::stdout().flush().unwrap();
        let mut cmd = String::new();
        io::stdin().read_line(&mut cmd).unwrap();
    }
}
