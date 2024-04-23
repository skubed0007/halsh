use clearscreen::*;
use colored::*;
use std::{
    env::{current_dir, set_current_dir},
    fs,
    io::{self, Write},
};
use sys_info::{
    cpu_num, cpu_speed, disk_info, linux_os_release, mem_info, os_release, os_type, proc_total,
};

fn main() {
    clear().unwrap();
    const VER: &str = "1.0.0";

    match os_type() {
        Ok(os) => match os.as_str() {
            "Linux" => {
                println!(
                    "{}{}",
                    "Welcome to Halsh!\nA Beautiful Shell For Linux!\nHalsh Version - ".white(),
                    VER.white()
                );
            }
            "Windows" => {
                println!(
                    "{}{}",
                    "Welcome to Halsh!\nA Beautiful Shell For Windows!\nHalsh Version - ", VER
                );
            }
            _ => println!("Well, yeah!"),
        },
        Err(os) => {
            println!("Failed Getting Os Type : {}", os);
        }
    }

    loop {
        let currdir = current_dir().unwrap();
        let mut _curdir = "";
        if currdir.to_str().unwrap() == "/home" {
            _curdir = "~/";
            print!("{:?} : ", _curdir);
        } else {
            print!("{:?} : ", currdir);
        }

        io::stdout().flush().unwrap();
        let mut cmdd = String::new();
        io::stdin().read_line(&mut cmdd).unwrap();
        let cmd = cmdd.trim();

        match cmd {
            "neofetch" => {
                if let Ok(ostype) = os_type() {
                    let osrelease = os_release().unwrap();
                    let memusage = mem_info().unwrap();
                    let cpunum = cpu_num().unwrap();
                    let cpuspeed = cpu_speed().unwrap();
                    let proctotal = proc_total().unwrap();
                    let diskinfo = disk_info().unwrap();
                    let losr = linux_os_release().unwrap();

                    if ostype == "Linux" {
                        println!("{}{}", "OS Type -> ".yellow(), ostype.yellow());
                        println!("{}{}", "Kernel -> ".yellow(), osrelease.yellow());
                        println!(
                            "{}{}",
                            "Linux OS Release Info -> ".yellow(),
                            losr.pretty_name.unwrap_or_default().yellow()
                        );
                        println!(
                            "{}{:.2} GB",
                            "Memory Usage -> ".yellow(),
                            memusage.total as f64 / 1024.0 / 1024.0
                        );
                        println!("{}{}", "CPU Cores -> ".yellow(), cpunum);
                        println!("{}{} MHz", "CPU Speed -> ".yellow(), cpuspeed);
                        println!("{}{}", "Current Processes -> ".yellow(), proctotal);
                        println!(
                            "{}{:.2} GB Free / {:.2} GB Total",
                            "About Disk -> ".yellow(),
                            diskinfo.free as f64 / 1024.0 / 1024.0,
                            diskinfo.total as f64 / 1024.0 / 1024.0
                        );
                    } else {
                        println!("{}{}", "OS Type -> ".yellow(), ostype.yellow());
                        println!("{}{}", "Kernel -> ".yellow(), osrelease.yellow());
                        //println!("{}{}", "Linux OS Release Info -> ".yellow(), losr.pretty_name.unwrap_or_default().yellow());
                        println!(
                            "{}{:.2} GB",
                            "Memory Usage -> ".yellow(),
                            memusage.total as f64 / 1024.0 / 1024.0
                        );
                        println!("{}{}", "CPU Cores -> ".yellow(), cpunum);
                        println!("{}{} MHz", "CPU Speed -> ".yellow(), cpuspeed);
                        println!("{}{}", "Current Processes -> ".yellow(), proctotal);
                        println!(
                            "{}{:.2} GB Free / {:.2} GB Total",
                            "About Disk -> ".yellow(),
                            diskinfo.free as f64 / 1024.0 / 1024.0,
                            diskinfo.total as f64 / 1024.0 / 1024.0
                        );
                    }
                }
            }
            "cd" => {
                println!("Path : ");
                let mut path = String::new();
                io::stdin().read_line(&mut path).unwrap();
                set_current_dir(path.trim().to_string()).unwrap();
            }
            "clear" | "clr" => {
                clear().unwrap();
            }
            "ls" | "dir" => {
                let currdir = current_dir().unwrap();
                //let homedir = home_dir().unwrap();
                let mut _curdir = currdir.display().to_string();
                if let Ok(lsr) = fs::read_dir(&currdir) {
                    for entry in lsr {
                        if let Ok(entry) = entry {
                            println!("{:?}", entry.file_name().to_string_lossy().green());
                        }
                    }
                } else {
                    println!("Failed to read directory.");
                }
            }

            _ => println!("{}{}", "ERROR NO COMMAND FOUND FOR - ".red(), cmd.green()),
        }
    }
}
