if (process.env.npm_config_build_from_source) {
  process.exit(1);
}

try {
  require('..');
} catch (e) {
  process.exit(1);
}
