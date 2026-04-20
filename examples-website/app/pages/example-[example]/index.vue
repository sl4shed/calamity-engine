<script setup>
import { codeToHtml } from 'shiki';

const route = useRoute();
const exampleId = route.params.example;

const highlightedCode = ref('');
const canvasRef = ref(null);
const moduleInstance = ref(null);

const { data: manifest } = await useFetch(
  `examples/${exampleId}/manifest.json`,
  { server: false }
);

// highlight code safely
watchEffect(async () => {
  if (!manifest.value?.sourceFiles) return;

  const firstFile = Object.values(manifest.value.sourceFiles)[0];
  if (!firstFile) return;

  highlightedCode.value = await codeToHtml(firstFile, {
    lang: 'cpp',
    theme: 'slack-dark'
  });
});

let isLoaded = false;

// wait until canvas is REAL DOM node
const waitForCanvas = () => {
  return new Promise((resolve) => {
    const check = () => {
      if (canvasRef.value instanceof HTMLCanvasElement) {
        resolve(canvasRef.value);
      } else {
        requestAnimationFrame(check);
      }
    };
    check();
  });
};

const loadModule = async () => {
  if (!manifest.value || isLoaded) return;
  isLoaded = true;

  const canvas = await waitForCanvas();
  console.log(canvas);
  const basePath = `/examples/${exampleId}/build-web`;

  // IMPORTANT: reset previous runtime (HMR safety)
  window.Module = {
    canvas,
    print: console.log,
    printErr: console.error,
    setStatus: (t) => console.log('[WASM]', t),
    locateFile: (path) => `${basePath}/${path}`,
  };

  await new Promise((resolve, reject) => {
    const script = document.createElement('script');
    script.src = `${basePath}/index.js`;

    // IMPORTANT: no async (Emscripten hates race conditions)
    script.async = false;

    script.onload = resolve;
    script.onerror = reject;

    document.body.appendChild(script);
  });

  moduleInstance.value = window.Module;
};

onMounted(() => {
  requestAnimationFrame(async () => {
    if (!canvasRef.value) return;

    canvasRef.value.width = manifest.value?.width || 800;
    canvasRef.value.height = manifest.value?.height || 600;

    await loadModule();
  });
});

// HMR cleanup (THIS is why you needed multiple refreshes)
if (import.meta.hot) {
  import.meta.hot.dispose(() => {
    window.Module = null;
    isLoaded = false;
  });
}

const exampleTitle = computed(() => {
  return (
    manifest.value?.name ||
    exampleId.replace(/_/g, ' ').replace(/\b\w/g, l => l.toUpperCase())
  );
});
</script>

<template>
  <div>
    <UPageHeader :title="exampleTitle">
      <template #headline>
        <UBreadcrumb :items="[
          { label: 'Examples', to: '/examples' },
          { label: exampleTitle }
        ]" />
      </template>
    </UPageHeader>


  <UPageBody>
    <div class="flex flex-col lg:flex-row gap-6">
      
      <div class="lg:w-2/3">
        <div class="bg-gray-900 dark:bg-gray-950 rounded-lg overflow-hidden">
          <canvas 
            ref="canvasRef"
            class="w-full h-auto"
            :width="manifest?.width || 800"
            :height="manifest?.height || 600"
            @contextmenu.prevent
          />
        </div>
      </div>
      
      <div class="lg:w-1/3">
        <UCard>
          peanits peanits balls
        </UCard>
      </div>
    </div>
  </UPageBody>
  </div>
</template>