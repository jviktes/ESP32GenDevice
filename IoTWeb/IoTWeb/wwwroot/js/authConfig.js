/**
 * Configuration object to be passed to MSAL instance on creation. 
 * For a full list of MSAL.js configuration parameters, visit:
 * https://github.com/AzureAD/microsoft-authentication-library-for-js/blob/dev/lib/msal-browser/docs/configuration.md
 * For more details on MSAL.js and Azure AD B2C, visit:
 * https://github.com/AzureAD/microsoft-authentication-library-for-js/blob/dev/lib/msal-browser/docs/working-with-b2c.md 
 */

const msalConfig = {
    auth: {
        clientId: "4d80b026-d71b-48de-9a2b-9be5801635be", // This is the ONLY mandatory field; everything else is optional.
        authority: "https://login.microsoftonline.com/815bbad6-b446-4ee6-a6cc-0d5880afd740", // Choose sign-up/sign-in user-flow as your default.
        redirectUri: "https://localhost:7057", // You must register this URI on Azure Portal/App Registration. Defaults to "window.location.href".
    },
    cache: {
        cacheLocation: "sessionStorage", // Configures cache location. "sessionStorage" is more secure, but "localStorage" gives you SSO memoryStorage
        storeAuthStateInCookie: true, // If you wish to store cache items in cookies as well as browser cache, set this to "true".
    },
};
const apiConfig = {
    //scopes: ["api://42e8e1b7-0c93-4dcf-b29a-99d189516179/access_data"],
    scopes: ["openid", "profile", "User.Read"],
    uri: "https://localhost:44332/api/todolist"
};

/**
 * Scopes you add here will be prompted for user consent during sign-in.
 * By default, MSAL.js will add OIDC scopes (openid, profile, email) to any login request.
 * For more information about OIDC scopes, visit: 
 * https://docs.microsoft.com/azure/active-directory/develop/v2-permissions-and-consent#openid-connect-scopes
 */
const loginRequest = {
    scopes: ["openid", ...apiConfig.scopes],
};

/**
 * Scopes you add here will be used to request a token from Azure AD B2C to be used for accessing a protected resource.
 * To learn more about how to work with scopes and resources, see: 
 * https://github.com/AzureAD/microsoft-authentication-library-for-js/blob/dev/lib/msal-browser/docs/resources-and-scopes.md
 */
const tokenRequest = {
    scopes: [...apiConfig.scopes],  // e.g. ["https://fabrikamb2c.onmicrosoft.com/helloapi/demo.read"]
    forceRefresh: false // Set this to "true" to skip a cached token and go to the server to get a new token
};

// exporting config object for jest
if (typeof exports !== 'undefined') {
    module.exports = {
        msalConfig: msalConfig,
    };
}