
extern crate anyhow;

use anyhow::Result;
use anyhow::bail;
use anyhow::Context;

/// multiply {x} by two assuming it is between 0 and 10 inclusive
fn a(x: i32) -> Result<i32>
{
    (|| {
	if x < 0 {
	    bail!("x < 0");
	}
	if x > 10 {
	    bail!("x > 10");
	}
	Ok(x * 2)
    }
    )().with_context(
	||format!("failed to multiply {x} by two assuming it is between 0 and 10 inclusive because", x=x))
}

/// calculate a({x}) - 1
fn b(x: i32) -> Result<i32>
{
    (|| -> Result<i32> {
	Ok(a(x)? - 1)
    }
    )().with_context(
	||format!("failed to calculate a({x}) - 1 because", x=x))
}

fn main()
{
    b(11).context("failed to run test because").unwrap();
}
