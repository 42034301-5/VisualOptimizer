import axios from 'axios'

// 每次请求携带cookies信息
axios.defaults.headers.post['Content-Type'] = 'application/json'
axios.defaults.withCredentials = true

export function getDomainUrl() {
    return 'http://localhost:8081'
}

// create an axios instance
const service = axios.create({
  // baseURL:'https://api.guisu.fun:6001/api/',
  baseURL: getDomainUrl() + '/api',
  timeout: 50000,
  async:true,
  crossDomain:true,
})

// request interceptor
service.interceptors.request.use(config => {
        config.headers = {
            'Content-Type': 'application/json'
        }
        return config;
    },error => {
        return Promise.reject(error)
    }
)

service.interceptors.response.use(response => {

        if (response.status != 200) {
            return Promise.reject(new Error(response))
        } else {
            return response
        }
    },
    error => {
        return Promise.reject(error)
    }
)

export default service