use std::env;
use std::process;

mod commands;
mod app_args;
use commands::help::run_help_comand;
use app_args::AppArgs;

fn die(s: &str) {
    println!("die: {}", s);
    process::exit(1);
}

fn parse_app_args(args: &Vec<String>) -> Option<AppArgs> {
    let mut app_args: AppArgs = AppArgs {
        args: args,
        is_help_command: false,
    };

    if args.len() < 2 {
        app_args.is_help_command = true;
        return Some(app_args);
    }

    let cmd: &String = &args[1];

    match cmd.as_str() {
        "help" => app_args.is_help_command = true,
        _ => return None,
    }

    Some(app_args)
}

fn run_app(args: &Vec<String>) {
    let result: Option<AppArgs> = parse_app_args(&args);
    if result == None {
        die("failed to parse options");
    }
    let app_args: AppArgs = result.unwrap();

    if app_args.is_help_command {
        run_help_comand(&app_args);
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    run_app(&args);
}
