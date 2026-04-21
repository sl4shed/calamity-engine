// insane build script

import fs from 'fs';
import path from 'path';

const examplesDir = path.join(import.meta.dirname, '../examples/');
const junkFiles = ['cmake_install.cmake', 'CMakeCache.txt', 'compile_commands.json', 'CPackConfig.cmake', 'CPackSourceConfig.cmake', 'Makefile'];
const junkDirs = ['build', 'build-psp'];
let totalManifest = {"examples": []};

fs.readdir(examplesDir, (err, files) => {
  if (err) {
    console.error('Error reading examples directory:', err);
    return;
  }

  files.forEach(file => {
    console.log("----------------------------------------------");
    const examplePath = path.join(examplesDir, file);
    if (fs.lstatSync(examplePath).isDirectory()) {
      const manifestPath = path.join(examplePath, 'manifest.json');
      const thumbnailPath = path.join(examplePath, 'thumbnail.png');

      if (fs.existsSync(manifestPath) && fs.existsSync(thumbnailPath)) {
        const buildDir = path.join(import.meta.dirname, '../examples', file, 'build-web');
        if(!fs.existsSync(buildDir)) {
            console.warn(`${file} => Emscripten build directory does not exist.`);
            return;
        }

        fs.copyFileSync(manifestPath, path.join(buildDir, 'manifest.json'));
        fs.copyFileSync(thumbnailPath, path.join(buildDir, 'thumbnail.png'));
        console.log(`${file} -> Copied thumbnail and manifest to build-web`);
        const manifestFile = JSON.parse(fs.readFileSync(manifestPath, 'utf-8'));

        const examplesDirPath = path.join(import.meta.dirname, 'public', 'examples', file);
        fs.mkdirSync(examplesDirPath, { recursive: true });
        fs.cpSync(path.join(examplesDir, file), examplesDirPath, { recursive: true });
        console.log(`${file} -> Copied to public/examples/${file}`);

        fs.readdir(examplesDirPath, (err, buildFiles) => {
            for (const junkFile of junkFiles) {
                const junkFilePath = path.join(examplesDirPath, 'build-web', junkFile);
                if (fs.existsSync(junkFilePath)) {
                    fs.unlinkSync(junkFilePath);
                    console.log(`${file} -> Deleted junk file: ${junkFile}`);
                }
            }

            for (const junkDir of junkDirs) {
                const junkDirPath = path.join(examplesDirPath, 'build-web', junkDir);
                if (fs.existsSync(junkDirPath)) {
                    fs.rmSync(junkDirPath, { recursive: true });
                    console.log(`${file} -> Deleted junk directory: ${junkDir}`);
                }
            }
        });

        totalManifest.examples.push(manifestFile);
      } else {
        console.warn(`${file} => skipping because of missing manifest.json or thumbnail.png`);
      }
    }
  });
});

fs.writeFileSync(path.join(import.meta.dirname, 'public', 'examples', 'manifest.json'), JSON.stringify(totalManifest, null, 2));
console.log('Total manifest.json created with all examples.');