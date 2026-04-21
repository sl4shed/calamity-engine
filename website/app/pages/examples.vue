<script setup>

const { data: manifest } = await useFetch(
  `/examples/manifest.json`,
  { server: false }
);

const examples = computed(() => manifest.value.examples ?? []);

const blogPosts = computed(() => {
  return examples.value.map((example) => ({
    title: example.title,
    image: '/examples/' + example.id + '/thumbnail.png',
    to: '/example-' + example.id,
    badge: example.tags.join(", ")
  }));
});
</script>

<template>
  <UContainer>
    <div class="">
      <UPageHeader title="Calamity Engine Examples" />
      <UContainer>
        <UBlogPosts :posts="blogPosts" />
      </UContainer>
    </div>
  </UContainer>
</template>