/*
* Tencent is pleased to support the open source community by making Puerts available.
* Copyright (C) 2020 THL A29 Limited, a Tencent company.  All rights reserved.
* Puerts is licensed under the BSD 3-Clause License, except for the third-party components listed in the file 'LICENSE' which may be subject to their corresponding license terms.
* This file is subject to the terms and conditions defined in file 'LICENSE', which is part of this source code package.
*/

declare module "puerts" {
    import {Object, Class, $Delegate} from "ue"
    
    interface $Ref<T> {
        __doNoAccess: T
    }

    interface $InRef<T> {
        __doNoAccess: T
    }
    
    type $Nullable<T> = T | null;
    
    // --> modified by kg begin
    // songfuhao: 暴露registerBuildinModule接口用于注册内置模块
    function registerBuildinModule(name: string, module: any): void;
    // --< end
    
    // --> modified by kg begin
    // songfuhao: 保存未修改前的 console，以便于接入devtools相关接口逻辑
    const console_org: Console;
    // --< end

    type cstring = string | ArrayBuffer;

    function toCString(str:string) : ArrayBuffer;

    function toCPtrArray(...ab:ArrayBuffer[]) : ArrayBuffer;
    
    function $ref<T>(x? : T) : $Ref<T>;
    
    function $unref<T>(x: $Ref<T> | $InRef<T>) : T;
    
    function $set<T>(x: $Ref<T> | $InRef<T>, val:T) : void;
    
    const argv : {
        getByIndex(index: number): Object;
        getByName(name: string): Object;
    }
    
    function merge(des: {}, src: {}): void;
    
    //function requestJitModuleMethod(moduleName: string, methodName: string, callback: (err: Error, result: any)=> void, ... args: any[]): void;
    
    /**
     * @deprecated please use mixin instead! 
    */
    function makeUClass(ctor: { new(): Object }): Class;
    
    function blueprint<T extends {
        new (...args:any[]): Object;
    }>(path:string): T;

    namespace blueprint {
        type MixinConfig = { objectTakeByNative?:boolean, inherit?:boolean, generatedClass?: Class, noMixinedWarning?:boolean};
        function tojs<T extends typeof Object>(cls:Class): T;
        function mixin<T extends typeof Object, R extends InstanceType<T>>(to:T, mixinMethods:new (...args: any) => R, config?: MixinConfig) : {
            new (Outer?: Object, Name?: string, ObjectFlags?: number) : R;
            StaticClass(): Class;
        };
        function unmixin<T extends typeof Object>(to:T): void
        function load(cls: any): void
        function unload(cls: any): void
    }
    
    function on(eventType: string, listener: Function, prepend?: boolean) : void;
    
    function off(eventType: string, listener: Function) : void;
    
    function emit(eventType: string, ...args:any[]) : boolean;
    
    function toManualReleaseDelegate<T extends (...args: any) => any>(func: T): $Delegate<T>;
    
    function releaseManualReleaseDelegate<T extends (...args: any) => any>(func: T): void;
    
    function toDelegate<T extends Object, K extends keyof T>(obj: T, key: T[K] extends (...args: any) => any ? K : never) : $Delegate<T[K] extends (...args: any) => any ? T[K] : never>;

    /*function getProperties(obj: Object, ...propNames:string[]): any;
    function getPropertiesAsync(obj: Object, ...propNames:string[]): Promise<any>;
    function setProperties(obj: Object, properties: any):void;
    function setPropertiesAsync(obj: Object, properties: any):Promise<void>;
    function flushAsyncCall(trace?:boolean):number;

    type AsyncFunction<T extends (...args: any) => any>  = (...a: ArgumentTypes<T>) => Promise<ReturnType<T> extends Object ? AsyncObject<ReturnType<T>> : ReturnType<T>>;

    type AsyncObject<T> = {
        [P in keyof T] : T[P] extends (...args: any) => any ? AsyncFunction<T[P]> : T[P];
    } & T

    function $async<T>(x: T) : AsyncObject<T>;*/
}

// --> modified by kg begin
// songfuhao: 保留 require，解决编译问题，引入 types/node 问题比较多
declare function require(name: string): any;
// --< end

// --> modified by kg begin
// liangcheng: 因为require时puerts调用时传入了__filename以及__dirname，所以要使用他们得加个声明
declare const __filename: string;
declare const __dirname: string;
// --< end
