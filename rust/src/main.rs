use std::env;
use std::path::PathBuf;
use std::fs::{File, OpenOptions};
use std::io::{self, Write, Read};
use clap::{Parser, Subcommand};

/// The name of the file where bookmarks are stored.
const BM_FILENAME: &str = ".bookmarks";
/// The delimiter used to separate the bookmark name from the path.
const DELIM: &str = "|";

#[derive(Debug, PartialEq, Eq, Clone, Copy)]
enum FileMode {
    Read,
    Append,
    Write
}

#[derive(Parser)]
#[command(author, version, about, long_about = None)]
struct Cli {
    #[command(subcommand)]
    command: Commands,
}

#[derive(Subcommand)]
enum Commands {
    /// Add a bookmark.
    Add { name: String },
    /// List all bookmarks.
    List,
    /// Find a bookmark.
    Find { name: String },
    /// Pop the last added bookmark.
    Pop,
}

/// Gets the path to the bookmarks file.
/// Use the environment variable `WDC_BOOKMARK_FILE` if it's defined.
/// Otherwise, look in the user's home directory for `BM_FILENAME`.
/// If there is no `HOME` directory, then look in the current directory.
fn get_bookmark_path() -> PathBuf {
    env::var("WDC_BOOKMARK_FILE")
	.map(PathBuf::from)
	.unwrap_or_else(|_| {
	    env::home_dir()
		.or_else(|| env::current_dir().ok())
		.unwrap_or_else(|| ".".into())
		.join(BM_FILENAME)
	})
}

/// Open the bookmark file.
fn open_bookmark_file(mode: FileMode) -> io::Result<File> {
    let path = get_bookmark_path();
    let mut options = OpenOptions::new();
    match mode {
	FileMode::Read => options.read(true),
	FileMode::Append => options.append(true).create(true),
	FileMode::Write => options.write(true).create(true).truncate(true),
    };
    match options.open(&path) {
	Ok(file) => Ok(file),
	Err(e) => {
	    eprintln!("Error: Could not open bookmarks file '{}'.", path.display());
	    Err(e)
	}
    }
}

/// Add bookmark to file with name and the path separated by DELIM.
fn add_to_file(name: &str, cwd_path: &str, mut bookmark_file: &File) -> io::Result<()> {
    writeln!(bookmark_file, "{}{}{}", name, DELIM, cwd_path)
}

/// Add bookmark with name, name.
fn add(name: &str) -> io::Result<()> {
    let cwd = env::current_dir()?;
    let bookmark_file = open_bookmark_file(FileMode::Append)?;
    add_to_file(name, cwd.to_str().unwrap(), &bookmark_file)
}

fn get_bookmarks() -> io::Result<Vec<String>> {
    let mut file = open_bookmark_file(FileMode::Read)?;
    let mut contents = String::new();
    file.read_to_string(&mut contents)?;
    Ok(contents.lines().map(String::from).collect())
}

fn get_bookmarks_reversed() -> io::Result<Vec<String>> {
    let mut bookmarks = get_bookmarks()?;
    bookmarks.reverse();
    Ok(bookmarks)
}

fn find(needle: &str) -> io::Result<Option<String>> {
    let bookmarks = get_bookmarks_reversed()?;
    for bookmark in bookmarks {
        let parts: Vec<&str> = bookmark.split(DELIM).collect();
        if parts.len() == 2 && parts[0] == needle {
            return Ok(Some(parts[1].to_string()));
        }
    }
    Ok(None)
}

fn pop() -> io::Result<Option<String>> {
    let mut bookmarks = get_bookmarks_reversed()?;
    if bookmarks.is_empty() {
        return Ok(None);
    }

    let popped_bookmark = bookmarks.remove(0);
    bookmarks.reverse(); // Write back in original order

    let mut file = open_bookmark_file(FileMode::Write)?;
    for bookmark in bookmarks {
        writeln!(file, "{}", bookmark)?;
    }

    let parts: Vec<&str> = popped_bookmark.split(DELIM).collect();
    if parts.len() == 2 {
        Ok(Some(parts[1].to_string()))
    } else {
        Ok(None) // Should not happen with well-formed data
    }
}

fn list_bookmarks() -> io::Result<()> {
    let bookmarks = get_bookmarks_reversed()?;
    for bookmark in bookmarks {
        println!("{}", bookmark);
    }
    Ok(())
}

fn main() {
    let cli = Cli::parse();

    match &cli.command {
        Commands::Add { name } => {
            if let Err(e) = add(name) {
                eprintln!("Failed to add bookmark: {}", e);
            }
        }
        Commands::List => {
            if let Err(e) = list_bookmarks() {
                eprintln!("Error listing bookmarks: {}", e);
            }
        }
        Commands::Find { name } => {
            match find(name) {
                Ok(Some(path)) => println!("{}", path),
                Ok(None) => println!("Bookmark not found."),
                Err(e) => eprintln!("Error finding bookmark: {}", e),
            }
        }
        Commands::Pop => {
            match pop() {
                Ok(Some(path)) => println!("{}", path),
                Ok(None) => println!("No bookmarks to pop."),
                Err(e) => eprintln!("Error popping bookmark: {}", e),
            }
        }
    }
}
