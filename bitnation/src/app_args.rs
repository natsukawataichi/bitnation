#[derive(PartialEq)]
pub struct AppArgs<'a> {
    pub args: &'a Vec<String>,
    pub is_help_command: bool,
}

