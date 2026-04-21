import fs from 'fs';
import path from 'path';

const rootDir = import.meta.dirname;
const examplesDir = path.join(rootDir, '../examples');

const junkFiles = [
  'cmake_install.cmake',
  'CMakeCache.txt',
  'compile_commands.json',
  'CPackConfig.cmake',
  'CPackSourceConfig.cmake',
  'Makefile'
];

const junkDirs = ['build', 'build-psp'];

const totalManifest = { examples: [] };

const files = fs.readdirSync(examplesDir);

for (const file of files) {
  console.log('----------------------------------------------');

  const examplePath = path.join(examplesDir, file);

  if (!fs.lstatSync(examplePath).isDirectory()) continue;

  const manifestPath = path.join(examplePath, 'manifest.json');
  const thumbnailPath = path.join(examplePath, 'thumbnail.png');

  if (!fs.existsSync(manifestPath) || !fs.existsSync(thumbnailPath)) {
    console.warn(`${file} => missing manifest.json or thumbnail.png`);
    continue;
  }

  const buildDir = path.join(examplePath, 'build-web');

  if (!fs.existsSync(buildDir)) {
    console.warn(`${file} => missing build-web directory`);
    continue;
  }

  // copy the manifest and thumbnail to build-web
  fs.copyFileSync(manifestPath, path.join(buildDir, 'manifest.json'));
  fs.copyFileSync(thumbnailPath, path.join(buildDir, 'thumbnail.png'));

  console.log(`${file} -> copied manifest + thumbnail to build-web`);

  const manifestFile = JSON.parse(fs.readFileSync(manifestPath, 'utf-8'));

  // copy example to public folder
  const publicExamplePath = path.join(rootDir, 'public', 'examples', file);
  fs.mkdirSync(publicExamplePath, { recursive: true });

  fs.cpSync(examplePath, publicExamplePath, { recursive: true });

  console.log(`${file} -> copied to public/examples/${file}`);

  const buildWebPath = path.join(publicExamplePath, 'build-web');

  // clean up
  for (const junkFile of junkFiles) {
    const junkPath = path.join(buildWebPath, junkFile);
    if (fs.existsSync(junkPath)) {
      fs.unlinkSync(junkPath);
      console.log(`${file} -> deleted junk file: ${junkFile}`);
    }
  }

  // clean up
  for (const junkDir of junkDirs) {
    const junkPath = path.join(buildWebPath, junkDir);
    if (fs.existsSync(junkPath)) {
      fs.rmSync(junkPath, { recursive: true, force: true });
      console.log(`${file} -> deleted junk dir: ${junkDir}`);
    }
  }

  // add to total manifest
  totalManifest.examples.push({
    title: manifestFile.title,
    tags: manifestFile.tags,
    id: file
  });
}

// write examples manifest
const outputManifestPath = path.join(rootDir, 'public', 'examples', 'manifest.json');

fs.mkdirSync(path.dirname(outputManifestPath), { recursive: true });
fs.writeFileSync(outputManifestPath, JSON.stringify(totalManifest, null, 2));

console.log('----------------------------------------------');