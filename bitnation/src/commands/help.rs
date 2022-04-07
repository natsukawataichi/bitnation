use std::process;

use crate::app_args::AppArgs;

pub fn run_help_comand(_: &AppArgs) {
    println!(
        "BitNation is the tool for management the digital nation.

Usage:

    $ bitnation [command] [arguments] [options]...

The commands are:

    help    Show usage.
"
    );
    process::exit(0);
}

