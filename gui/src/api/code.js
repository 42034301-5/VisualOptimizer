import request from '@/utils/request'

export function InitWithCode(codes) {
    return request({
        url: '/init',
        method: 'get',
        params: {codes:codes}
    })
}

export function ReadLine(lineNumber){
    return request({
        url:'read/line',
        method: 'get',
        params: {lineNumber:lineNumber}
    })
}