export default defineNuxtPlugin((nuxtApp) => {
    const router = nuxtApp.$router;

    // @ts-ignore
    router.beforeEach((to, from) => {
        if (
            to.path.startsWith('/example-') &&
            from.path !== to.path
        ) {
            // reload page if example
            window.location.href = to.fullPath;
            return false;
        }
    });
});