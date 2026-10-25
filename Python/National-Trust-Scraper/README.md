Python Project No. 2: Scraping National Trust Sites

# Wikipedia Rules for Scraping

1. **Consider if dumps are more efficient than live requests.**
2. **Accurately identify your User-Agent** as described Wikimedia Foundation User-Agent Policy
3. When you are making a significant number of requests **avoid user-agent impersonation** and possible collateral damage blocks by either:
    1. **Provide a URL** where we can download a JSON formatted list of CIDRs from which your requests will originate.
    2. **Authenticate your requests** using an on-wiki account when you are making API requests. An OAuth 2.0 access token is the preferred authentication method, but session cookies are also supported.
4. **Honor a website’s specified Robots.txt**. This can be found at `https://www.example.com/robots.txt` 
5. **Default to gzip**
6. **Respect our HTTP status codes.** When we reply with a 429 Too Many Requests status code respect the delay specified by the Retry-After header sent with the response.
7. **Cached interfaces are preferred and more efficient.** If you need the HTML content of pages use either the `/wiki/Article_name` URL or the corresponding Wikimedia REST API `/api/rest_v1/page/html/Article_name` endpoint.

# Discussion of Rules

A recurring issue is `HTTP Status Code 403: Too many requests. Please respect our robot policy https://w.wiki/4wJS` (described above). It’s quite difficult to identify what the exact problem is, especially as the error only arises sometimes, but going through each rule/guideline:

1. Dumps are **NOT** viable, as they are too large files (can be in the TB range)
2. I have found it very difficult to know what to put for the `user-agent`. Turns out, when you use the browser to search something, it automatically generates header information including your `user-agent` . However, when using Python, it doesn’t have to match your computer, it just has to be realistic and consistent with the other headers.
3. I am **NOT** making a significant number of requests (which they define >5 requests per second as “high-volume”)
4. I have skimmed through `https://en.wikipedia.org/robots.txt` but nothing stood out
5. This is included in the `headers` with `{'accept-encoding': 'gzip'}`
6. Only `HTTP Status Code 403` has come up. There are checks anyway for `429` to respect the `Retry-After` delay
7. I am using the `/wiki/Article_name` URL

# The Solution

- It appears that in my attempts, I got IP blocked, and required a VPN to bypass the block. However, any attempts without the VPN proved moot, as even with the correct set up, the block would trigger a `403` error.
- After starting to use the VPN, I quickly realised 2 issues:
    - The headers need to look more natural (as described above in **Discussion of Rules, Point 2**)
    - Each `session` seems to store data (most likely cookies), that re-using that session triggers the **Wikipedia** servers to recognise my program as a bot. Thus, restarting a new `session` before each request solved this issue
- **NOTE**: Python `wikipedia` module kept failing
