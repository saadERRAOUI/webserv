import { parse, stringify } from "jsr:@std/toml";



console.log(parse(await Deno.readTextFile("config.toml")))