use std::env;
use std::path::{Path, PathBuf};
use std::fs::{File, OpenOptions};
use std::io::{self, BufRead, BufReader, Write};
use clap::{Parser, Subcommand};


/// Environment variable that can overwrite the bookmarks file path.
const BM_ENV: &str = "WDC_BOOKMARK_FILE";
/// The name of the file where bookmarks are stored.
const BM_FILENAME: &str = ".bookmarks";
/// The delimiter used to separate the bookmark name from the path.
const DELIM: &str = "|";

/// A bookmark is just a name and the directory it points to.
#[derive(Debug, Clone, PartialEq, Eq)]
struct Bookmark {
    name: String,
    path: String,
}

impl Bookmark {
    /// Create a new Bookmark from name and path.
    fn new(name: impl Into<String>, path: impl Into<String>) -> Self {
        Self {
            name: name.into(),
            path: path.into(),
        }
    }

    /// Parse a single line from the bookmarks file.
    fn from_line(line: &str) -> Option<Self> {
        let (name, path) = line.split_once(DELIM)?;
        Some(Self::new(name, path))
    }

    /// Serialise back to the line format used in the file.
    fn to_line(&self) -> String {
        format!("{}{}{}", self.name, DELIM, self.path)
    }
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
/// Use the environment variable BM_ENV if it's defined.
/// Otherwise, look in the user's home directory for `BM_FILENAME`.
/// If there is no `HOME` directory, then look in the current directory.
fn bookmark_file_path() -> PathBuf {
    env::var(BM_ENV)
	.map(PathBuf::from)
	.unwrap_or_else(|_| {
	    env::home_dir()
		.or_else(|| env::current_dir().ok())
		.unwrap_or_else(|| ".".into())
		.join(BM_FILENAME)
	})
}

/// Load every bookmark in the file, newest → oldest.
/// Invalid lines are silently ignored.
fn load_bookmarks<P: AsRef<Path>>(path: P) -> io::Result<Vec<Bookmark>> {
    let file = File::open(path)?;
    let reader = BufReader::new(file);

    let mut bookmarks: Vec<_> = reader
        .lines()
        .filter_map(|line| line.ok().and_then(|l| Bookmark::from_line(&l)))
        .collect();

    bookmarks.reverse();
    Ok(bookmarks)
}

/// Add a bookmark for the current working directory.
fn add_bookmark(name: &str) -> io::Result<()> {
    let path = env::current_dir()?.display().to_string();
    let bookmark = Bookmark::new(name, path);

    let file_path = bookmark_file_path();
    let mut file = OpenOptions::new()
        .create(true)
        .append(true)
        .open(&file_path)?;
    writeln!(file, "{}", bookmark.to_line())?;
    Ok(())
}


/// List all bookmarks to stdout (newest first).
fn list_bookmarks() -> io::Result<()> {
    let bookmarks = load_bookmarks(bookmark_file_path())?;
    for bm in bookmarks {
        println!("{}{}{}", bm.name, DELIM, bm.path);
    }
    Ok(())
}

/// Persist the given bookmarks to disk, oldest → newest.
fn save_bookmarks<P: AsRef<Path>>(path: P, bookmarks: &[Bookmark]) -> io::Result<()> {
    let mut file = OpenOptions::new()
        .write(true)
        .create(true)
        .truncate(true)
        .open(path)?;
    for bm in bookmarks.iter().rev() {
        writeln!(file, "{}", bm.to_line())?;
    }
    Ok(())
}

/// Print the path for the given bookmark name.
fn find_bookmark(name: &str) -> io::Result<Option<String>> {
    let bookmarks = load_bookmarks(bookmark_file_path())?;
    Ok(bookmarks
        .into_iter()
        .find(|bm| bm.name == name)
        .map(|bm| bm.path))
}


/// Remove the newest bookmark and return its path.
fn pop_bookmark() -> io::Result<Option<String>> {
    let file_path = bookmark_file_path();
    let mut bookmarks = load_bookmarks(&file_path)?;

    let popped = bookmarks.first().cloned();
    bookmarks.remove(0);

    save_bookmarks(file_path, &bookmarks)?;
    Ok(popped.map(|bm| bm.path))
}

fn main() {
    let cli = Cli::parse();

    match &cli.command {
        Commands::Add { name } => {
            if let Err(e) = add_bookmark(name) {
                eprintln!("Failed to add bookmark: {}", e);
            }
        }
        Commands::List => {
            if let Err(e) = list_bookmarks() {
                eprintln!("Error listing bookmarks: {}", e);
            }
        }
        Commands::Find { name } => {
            match find_bookmark(name) {
                Ok(Some(path)) => println!("{}", path),
                Ok(None) => println!("Bookmark not found."),
                Err(e) => eprintln!("Error finding bookmark: {}", e),
            }
        }
        Commands::Pop => {
            match pop_bookmark() {
                Ok(Some(path)) => println!("{}", path),
                Ok(None) => println!("No bookmarks to pop."),
                Err(e) => eprintln!("Error popping bookmark: {}", e),
            }
        }
    }
}
