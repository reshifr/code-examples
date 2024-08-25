const { setTimeout } = require("timers/promises");

async function execCommand() {
  console.log("Run command.");
}

async function main() {
  const future = execCommand();
  await setTimeout(2000);
  console.log("Skip command.");
  await future;
}

main();
