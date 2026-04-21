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

const activeTab = ref('');
const fileTabItems = computed(() => {
  const files = manifest.value?.sourceFiles;
  if (!files) return [];

  return files.map((file) => ({
    label: file,
    value: file,
    icon: 'i-lucide-file'
  }));
});

watchEffect(() => {
  const file = activeTab.value;
  if (!file) return;

  (async () => {
    try {
      const res = await fetch(`/examples/${exampleId}/${file}`);
      const text = await res.text();

      highlightedCode.value = await codeToHtml(text, {
        lang: 'cpp',
        theme: 'slack-dark'
      });
    } catch (err) {
      console.error('Failed to load file:', file, err);
      highlightedCode.value = '';
    }
  })();
});

watchEffect(() => {
  const file = activeTab.value;
  if (!file) return;

  (async () => {
    const res = await fetch(`/examples/${exampleId}/${file}`);
    const text = await res.text();

    highlightedCode.value = await codeToHtml(text, {
      lang: 'cpp',
      theme: 'slack-dark'
    });
  })();
});

let isLoaded = false;

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
  const basePath = `/examples/${exampleId}/build-web`;

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

const exampleTitle = computed(() => {
  return (
    manifest.value?.name ||
    exampleId.replace(/_/g, ' ').replace(/\b\w/g, l => l.toUpperCase())
  );
});
</script>

<template>
  <UContainer>
    <UPageHeader :title="manifest?.title || exampleTitle">
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
          <div class="bg-transparent rounded-lg flex items-center justify-center">
            <canvas ref="canvasRef" id="canvas" :width="manifest.width" :height="manifest.height"
              @contextmenu.prevent />
          </div>
        </div>

        <div class="lg:w-1/3">
          <UCard>
            <div class="flex flex-col gap-4">
              <div class="flex flex-row gap-3">
                <UBadge v-for="tag in manifest.tags" :key="tag">
                  {{ tag }}
                </UBadge>
              </div>

              <USeparator />

              {{ manifest?.description }}

              <USeparator />

              <UDrawer title="Source Code" direction="right" :inset="true" :ui="{ width: 'w-1/3' }" size="lg">
                <UButton label="Source Code" icon="i-lucide-code" variant="subtle" color="neutral" />

                <template #content>
                  <UTabs v-model="activeTab" :items="fileTabItems" variant="link">
                    <template #default>
                      
                    </template>
                  </UTabs>
                </template>
              </UDrawer>
            </div>
          </UCard>
        </div>
      </div>
    </UPageBody>
  </UContainer>
</template>