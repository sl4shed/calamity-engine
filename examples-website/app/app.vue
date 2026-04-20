<script setup>
useHead({
  meta: [
    { name: 'viewport', content: 'width=device-width, initial-scale=1' }
  ],
  link: [
    { rel: 'icon', href: '/favicon.ico' }
  ],
  htmlAttrs: {
    lang: 'en'
  }
})

const title = 'Calamity Engine'
const description = 'A small, portable game engine written in C++ with SDL3.'

// useSeoMeta({
//   title,
//   description,
//   ogTitle: title,
//   ogDescription: description,
//   ogImage: 'https://ui.nuxt.com/assets/templates/nuxt/starter-light.png',
//   twitterCard: 'summary_large_image'
// })
</script>

<template>
  <UApp>
    <UHeader>
      <template #left>
        <NuxtLink to="/">
          <AppLogo class="w-auto h-12 shrink-0" />
        </NuxtLink>
      </template>

      <template #right>
        <UButton
          to="https://github.com/sl4shed/calamity-engine"
          target="_blank"
          icon="i-simple-icons-github"
          aria-label="GitHub"
          color="neutral"
          variant="ghost"
        />
      </template>
    </UHeader>

    <UMain>
      <NuxtPage />
    </UMain>

    <USeparator />

    <UFooter>
      <template #left>
        <p class="text-sm text-muted">
          Built with Nuxt UI • © {{ new Date().getFullYear() }}
        </p>
      </template>

      <template #right>
        <UButton
          to="https://github.com/sl4shed/calamity-engine"
          target="_blank"
          icon="i-simple-icons-github"
          aria-label="GitHub"
          color="neutral"
          variant="ghost"
        />
      </template>
    </UFooter>
  </UApp>
</template>
