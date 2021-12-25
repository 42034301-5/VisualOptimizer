import request from '@/utils/request'

export function InitWithCode(codes) {
    return request({
        url: '/init',
        method: 'post',
        data: {codes:codes}
    })
}

export function ReadLine(code){
    return request({
        url:'/readline',
        method: 'post',
        data: {code:code}
    })
}

export function RecordActive(actives){
    return request({
        url:'/recordActive',
        method: 'post',
        data: {actives:actives}
    })
}

export function Simplify(){
    return request({
        url:'/simplify',
        method: 'post'
    })
}